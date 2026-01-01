import { Link } from "react-router-dom";

export default function Navbar() {
  return (
    <nav className="bg-gray-900 text-white px-8 py-4 flex gap-6">
      <Link className="hover:text-green-400" to="/">Dashboard</Link>
      <Link className="hover:text-green-400" to="/register">Register User</Link>
      <Link className="hover:text-green-400" to="/create">Register IP</Link>
      <Link className="hover:text-green-400" to="/verify">Verify Ownership</Link>
      <Link className="hover:text-green-400" to="/chain">Blockchain</Link>
    </nav>
  );
}
